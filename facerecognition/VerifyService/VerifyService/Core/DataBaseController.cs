using System;
using System.Linq;
using VerifyService.DataBase;

namespace VerifyService.Core
{
    /// <summary>
    /// Класс для взаимодействия с базой данных
    /// </summary>
    public class DataBaseController : IDisposable
    {
        /// <summary>
        /// Контекст базы данных
        /// </summary>
        localdb4Entities _context;

        /// <summary>
        /// Контекст базы данных
        /// </summary>
        public localdb4Entities Context
        {
            get { return _context; }
            private set { _context = value; }
        }

        /// <summary>
        /// Переопределяем конструктор по-умолчанию
        /// </summary>
        public DataBaseController()
        {
            Context = new localdb4Entities();
            // отключаем "ленивую" загрузку
            Context.Configuration.LazyLoadingEnabled = false;
            // поключаемся к базе
            try
            {
                Context.Database.Connection.Open();
            }
            catch { throw new Exception("Ошибка подключения к базе данных"); }
        }

        /// <summary>
        /// Получить путь к видео и набор дескрипторов пользователя по id обследования
        /// </summary>
        /// <param name="idDiagnosis"></param>
        /// <returns>Item1 - путь к видео, Item2 - строка с дискрипторами, Item3 - Id пользователя</returns>
        public Tuple<string, string, long> GetUserVideo(string idDiagnosis)
        {
            // получаем обследование
            Guid id = new Guid(idDiagnosis);
            Diagnosis diagnosis = Context.Diagnosis.FirstOrDefault(d => d.id == id);
            // получаем пользователя
            Users user = Context?.Diagnosis.Include("Users").FirstOrDefault(x => x.id == id)?.Users;
            string path = "", disc = "";
            if (user != null && diagnosis != null)
            {
                path = diagnosis.videoPath;
                disc = Context.FaceDescriptor.FirstOrDefault(d => d.userId == user.id)?.descriptor;
                return new Tuple<string, string, long>(path, disc, user.id);
            }
            else
            {
                return null;
            }
        }

        public void Dispose()
        {
            // закрываем работы с базой
            Context?.Database.Connection.Close();
        }
    }
}
