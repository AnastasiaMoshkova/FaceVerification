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
    
    public partial class Subscriptions
    {
        public Nullable<long> structID { get; set; }
        public Nullable<long> userRoleID { get; set; }
        public Nullable<long> kppID { get; set; }
        public Nullable<long> firmID { get; set; }
        public long id { get; set; }
        public Nullable<long> reportSub { get; set; }
    
        public virtual Kpp Kpp { get; set; }
        public virtual Struct Struct { get; set; }
    }
}
