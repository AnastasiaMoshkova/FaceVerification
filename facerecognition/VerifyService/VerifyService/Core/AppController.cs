using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using VerifyService.DataBase;
using VerifyService.Enums;

namespace VerifyService.Core
{
    public class AppController : IDisposable
    {
        /// <summary>
        /// Контроллер базы данных
        /// </summary>
        private DataBaseController _dbController = new DataBaseController();

        /// <summary>
        /// Контроллер распознавания
        /// </summary>
        private RecognitionController _recController = new RecognitionController();

        /// <summary>
        /// Расчет дескрипторов по изображениям и сохранение их в базу (без перезаписи)
        /// </summary>
        /// <returns>Число обработанных и внесенных в базу дескрипторов</returns>
        public int Training()
        {
            // получение списка сотрудников
            List<Users> users = _dbController.Context.Users.Where(x => x.photoPath != "" && x.photoPath != null).ToList();
            // получение существующих записей в таблице дескрипторов
            List<long> trainedUser = _dbController.Context.FaceDescriptor.Select(y => y.userId).ToList();
            // счетчик обработанных записей
            int N = 0;
            foreach (Users u in users)
            {
                // если запись уже есть -> пропускаем
                if (trainedUser.Contains(u.id))
                    continue;
                try
                {
                    // получение для каждого сотрудника вектора
                    string vec = GetParamVector(u.photoPath);
                    // добавление вектора к контексту базы 
                    if (!string.IsNullOrWhiteSpace(vec))
                    {
                        _dbController.Context.FaceDescriptor.Add(new FaceDescriptor() { descriptor = vec, userId = u.id });
                        // сохраняем изменения в базе
                        _dbController.Context.SaveChanges();
                        N++;
                    }
                }
                catch (Exception ex) { throw new Exception($"Ошибка {ex.Message}"); }
            }
            return N;
        }

        /// <summary>
        /// Утилитный метод для определения порога
        /// </summary>
        /// <returns></returns>
        public double DefineThreshold()
        {
            // получаем все дескрипторы из базы
            var trainedUser = _dbController.Context.FaceDescriptor.Include("Users").ToList();
            DateTime time = DateTime.Now;
            using (TextWriter tw = new StreamWriter("params.csv", false))
            {
                // рассчитываем критерий каждого с каждым
                for (int i = 0; i < trainedUser.Count; i++)
                {
                    tw.Write(trainedUser[i].Users.id.ToString() + ";");
                    for (int k = 0; k < trainedUser.Count; k++)
                    {
                        double th = _recController.GetScalarProductDescriptors(trainedUser[i].descriptor, trainedUser[k].descriptor);
                        tw.Write(th.ToString("0.0000") + ";");
                    }
                    tw.WriteLine();
                    Console.WriteLine($"finished {i} from {trainedUser.Count}");
                }
            }
            double allTime = (DateTime.Now - time).TotalMinutes;
            return allTime;
        }      

        /// <summary>
        /// Верификация по видео
        /// </summary>
        /// <param name="idDiagnosis">Id обследования</param>
        /// <param name="th">Порог принятия решения о верификации</param>
        /// <returns>Item1 - подтверждено/не подтверждено, Item2 - имена предполагаемых сотрудников</returns>
        public Tuple<bool, List<string>> VerifyByVideo(string idDiagnosis, double th)
        {
            // получаем путь к видео и набор дескрипторов ожидаемого пользователя
            var pars = _dbController.GetUserVideo(idDiagnosis);
            if (pars == null)
            {
                Console.WriteLine("Not found instance in database");
                return null;
            }

            // проверяем существование ДЕСКРИПТОРОВ
            if (pars.Item2==null)
            {
                Console.WriteLine("Not found descriptors");
                return null;
            }

            // проверяем существование видео-файла
            if (!File.Exists(pars.Item1))
            {
                Console.WriteLine("Not found video file");
                return null;
            }

            // получаем обследование
            Diagnosis d = _dbController.Context.Diagnosis.Include("VerificationResult").First(diag => diag.id == new Guid(idDiagnosis));
            // запускаем поиск по видео
            var result = _recController.VerifyPerson(pars.Item1, th, pars.Item2);
            Console.WriteLine($"result facerec: {result.isVerify}, n of discriptors: { result.data.Count }");

            // если верификация прошла
            if (result.isVerify)
            {
                // записываем подтверждение обследования в базу
                d.verification = Convert.ToByte((int)VerifyResult.Verified);
                _dbController.Context.VerificationResult.Add(
                            new VerificationResult()
                            {
                                id = Guid.NewGuid(),
                                diagnosisId = new Guid(idDiagnosis),
                                personId = pars.Item3,
                                expectedId = pars.Item3,
                                criteria = (float)result.level
                            });
                _dbController.Context.SaveChanges();
                Console.WriteLine("Verified");
                return new Tuple<bool, List<string>>(true, new List<string>());
            }
            else
            {
                // больше одного лица
                if (result.num_face > 1)
                {
                    d.verification = Convert.ToByte((int)VerifyResult.MoreThenOneFace);
                    _dbController.Context.SaveChanges();
                    Console.WriteLine($"More then one face.");
                    return new Tuple<bool, List<string>>(false, null);
                    
                }
                else
                {

                    // если верификация не прошла и найдены лица
                    if (result.data?.Count > 0 || result.data == null)
                    {
                        // получаем из базы все дескрипторы и сравниваем
                        List<FaceDescriptor> trainedUser = _dbController.Context.FaceDescriptor.Include("Users").ToList();
                        List<ResultCompare> values = new List<ResultCompare>();
                        for (int i = 0; i < trainedUser.Count; i++)
                        {
                            values.Add(new ResultCompare()
                            {
                                Criteria = _recController.GetScalarProductDescriptors(trainedUser[i].descriptor, result.data[0]),
                                UserId = trainedUser[i].userId,
                                NameUser = trainedUser[i].Users.firstName + " " + trainedUser[i].Users.lastName
                            });
                        }
                        // если нет рассчитанных значений
                        if (values.Count < 1)
                        {
                            d.verification = Convert.ToByte((int)VerifyResult.NotFoundFace);
                            _dbController.Context.SaveChanges();
                            Console.WriteLine($"Unverified. Comparison with DB was failed.");
                            return new Tuple<bool, List<string>>(false, null);
                        }
                        // записываем, что нашли другого человека
                        d.verification = (int)VerifyResult.OtherPerson;
                        // сортируем
                        values = values.OrderBy(x => x.Criteria).ToList();
                        List<string> propableUsers = new List<string>();
                        // записываем первых трех вероятных в базу
                        for (int i = 0; i < 3; i++)
                        {
                            if (i > values.Count - 1)
                            {
                                break;
                            }
                            propableUsers.Add(values[i].NameUser);
                            _dbController.Context.VerificationResult.Add(
                                new VerificationResult()
                                {
                                    id = Guid.NewGuid(),
                                    diagnosisId = new Guid(idDiagnosis),
                                    personId = values[i].UserId,
                                    expectedId = pars.Item3,
                                    criteria = (float)values[i].Criteria,
                                    probability = (float)(1 - values[i].Criteria)
                                });
                        }
                        _dbController.Context.SaveChanges();
                        Console.WriteLine($"Unverified. Users were found.");
                        return new Tuple<bool, List<string>>(false, propableUsers);
                    }
                    // нет дескрипторов -> пишем в базу, что нет лиц
                    else
                    {
                        d.verification = Convert.ToByte((int)VerifyResult.NotFoundFace);
                        _dbController.Context.SaveChanges();
                        Console.WriteLine($"Unverified. Faces were not found");
                        return new Tuple<bool, List<string>>(false, null);
                    }
                }
            
            }
        }

        /// <summary>
        /// Получить вектор признаков по изображению
        /// </summary>
        /// <param name="pathImage">Путь к изображению</param>
        /// <returns>Вектор признаков в виде строки</returns>
        private string GetParamVector(string pathImage)
        {
            // проверяем существование файла
            if (!File.Exists(pathImage))
                return "";
            // строка с вектором
            string vec = "";
            try
            {
                vec = _recController.GetDescriptors(pathImage);
            }
            catch { throw new Exception("Ошибка в работе библиотеки распознавания."); }
            return vec;
        }


        public void VerifyByVideoAllDatabase(double th)
        {
            //List<string> idObserv = _dbController.Context.Diagnosis.Select(x => x.id.ToString()).ToList();
            //List<string> idObserv = _dbController.Context.Diagnosis.Select(x => x.id.ToString())/*.Take(10)*/.ToList();
            List<string> idObserv = _dbController.Context.VerificationQueue.Select(x => ((Guid)x.diagnosisId).ToString()).ToList();
           // List<string> idObserv = _dbController.Context.Diagnosis.Where(x => x.id == new Guid("11CFCBD5-A4A9-49C0-8550-416183015604")).
                                           // Select(y => y.id.ToString()).ToList();

        int n = 0;
            int nPassed = 0;
            foreach (string s in idObserv)
            {
                
                //AppController :
                DateTime time = DateTime.Now;
                var res = VerifyByVideo(s,th);
                double allTime = (DateTime.Now - time).TotalMilliseconds;
                //return allTime;
               

                if (res == null)
                {
                    nPassed++;
                    continue;
                }

                if (!res.Item1 && res.Item2 == null)
                    nPassed++;
                else
                {

                    Console.WriteLine($"processing {n + nPassed} in {idObserv.Count}: id = {s}");
                    Console.WriteLine(allTime);
                    n++;
                }
            }
        }

public void Dispose()
        {
            _dbController?.Dispose();
        }
    }
}
