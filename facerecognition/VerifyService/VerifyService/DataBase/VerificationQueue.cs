//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated from a template.
//
//     Manual changes to this file may cause unexpected behavior in your application.
//     Manual changes to this file will be overwritten if the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

namespace VerifyService.DataBase
{
    using System;
    using System.Collections.Generic;
    
    public partial class VerificationQueue
    {
        public long id { get; set; }
        public Nullable<System.Guid> diagnosisId { get; set; }
    
        public virtual Diagnosis Diagnosis { get; set; }
    }
}
