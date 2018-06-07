using System;
using System.Collections.Generic;
using facerecognition;

namespace VerifyService.Core
{
    /// <summary>
    /// Класс для взаимодействия с библиотекой распознавания
    /// </summary>
    public class RecognitionController
    {
        // Экземпляр класса распознавания лиц
        FaceRecDLib _recognizer = new FaceRecDLib();

        /// <summary>
        /// Определение вектора параметров по изображению
        /// </summary>
        /// <param name="path">Путь к изображению</param>
        /// <returns>Вектор параметров в виде строки</returns>
        public string GetDescriptors(string pathImage)
        {
            return _recognizer.face_descriptor_calc(pathImage);
        }

        /// <summary>
        /// Верификация пользователя по видео
        /// </summary>
        /// <param name="pathVideo">Путь к видео</param>
        /// <param name="threshold">Порог</param>
        /// <param name="vector">Вектор параметров, ожидаемого пользователя</param>
        /// <returns>Item1 - признак верификации ожидаемого пользователя, Item2 - набор векторов признаков найденных лиц (если верификация не прошла)</returns>
        public ResultVerify VerifyPerson(string pathVideo, double threshold, string vector)
        {
            return _recognizer.face_verification(vector, pathVideo, threshold);
        }

        /// <summary>
        /// Расчет евклидовой метрики (todo: или чего-то другого?)
        /// </summary>
        /// <param name="vec1">Первый вектор</param>
        /// <param name="vec2">Второй вектор</param>
        /// <returns>Евклидова метрика (todo: или что-то другое?)</returns>
        public double GetScalarProductDescriptors(string vec1, string vec2)
        {
            // расчет расстояния между векторами
            return _recognizer.descriptions_compare(vec1, vec2);
        }
    }
}
