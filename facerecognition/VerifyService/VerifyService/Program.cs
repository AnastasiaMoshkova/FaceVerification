using System;
using System.Linq;
using VerifyService.Core;

namespace VerifyService
{
    class Program
    {
        static void Main(string[] args)
        {
            string flag = args.Length > 0 ? args[0] : "";

            // создаем экзепляр контроллера
        
            AppController controller = new AppController();

            if (String.Compare(flag, "--verifyByvideo")==0)
            {
                // разбираем аргументы
                string id = args.Length > 1 ? args[1] : "";
                double threshold = args.Length > 2 ? double.Parse(args[2].Replace(".", ",")) : 0.6;
                bool notStop = args.Length > 3 ? true : false;

                if (string.IsNullOrWhiteSpace(id))
                    return;

                // создаем экзепляр контроллера
                // AppController controller = new AppController();


                // вызываем метод обработки
                var result = controller.VerifyByVideo(args[1], threshold);

                if (notStop)
                {
                    Console.WriteLine(result?.Item1);
                    if (result.Item2?.Count > 0)
                    {
                        foreach (string s in result.Item2)
                        {
                            Console.WriteLine(s);
                        }
                    }
                    Console.ReadLine();
                }
            }
            else if (String.Compare(flag, "--verifyByAllDatabase")==0)
            {

                double threshold = args.Length > 1 ? double.Parse(args[1].Replace(".", ",")) : 0.6;
                controller.VerifyByVideoAllDatabase(threshold);
            }

            else if (String.Compare(flag, "--calcDescriptors")==0)
            {
                // вызов обучения
                int n = controller.Training();
            }



            // вызов обучения
            // int n = controller.Training();

            // расчет порогов по всей базе
            // double time = controller.DefineThreshold();






            // закрываем базу
            controller.Dispose();

            Console.WriteLine($"Process is finished");
            Console.ReadLine();
        }
    }
}
