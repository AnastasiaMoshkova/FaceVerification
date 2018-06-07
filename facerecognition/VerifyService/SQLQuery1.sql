select * from VerificationResult v
join Diagnosis d on d.id = v.diagnosisId
where d.id =  'C236A4F3-E973-4717-BBBF-73EC003129C4'
order by d.diagnosisDateTime

select count(*) from Diagnosis d
join Users u on u.id = d.personID
join FaceDescriptor f on f.userId = u.id

select count(*) from FaceDescriptor

select * from Diagnosis d
join Users u on u.id = d.personID
where d.id = '0f6a1668-a475-480c-9fc8-001698520e0e'

select * from FaceDescriptor f
where f.userId = '1067131004'