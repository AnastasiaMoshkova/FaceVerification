using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace VerifyService.Core
{
    /// <summary>
    /// Результат сравнения дискрипторов по базе
    /// </summary>
    public class ResultCompare
    {
        /// <summary>
        /// Результат расчета расстояния
        /// </summary>
        public double Criteria = 1;
        /// <summary>
        /// Индекс в наборе пациентов
        /// </summary>
        public long UserId = -1;
        /// <summary>
        /// Имя пользователя
        /// </summary>
        public string NameUser = "";
    }
}
