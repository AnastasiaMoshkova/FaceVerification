using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace VerifyService.Enums
{
    /// <summary>
    /// Возможные результаты верификации с ключами для базы данных
    /// </summary>
    public enum VerifyResult
    {
        /// <summary>
        /// Подтверждено
        /// </summary>
        Verified,
        /// <summary>
        /// Лицо не найдено
        /// </summary>
        NotFoundFace,
        /// <summary>
        /// Другой сотрудник
        /// </summary>
        OtherPerson,
        /// <summary>
        /// Больше одного лица в кадре
        /// </summary>
        MoreThenOneFace,
    }
}
