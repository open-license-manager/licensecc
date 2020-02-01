# Backoffice (Draft) (Use Cases)

Not implemented yet. 
## Domain

**Software Provider**
* PK ProviderId
* Email (UNIQUE)
* Password
* LicenseGenerator?? (UNIQUE)
* Library headers and compiled files (zip).

**Product**
* PK ProductId + (FK) ProviderId

**Software User**
* PK UserId + (FK) ProviderId
* Email (UNIQUE)

**License**
* PK LicenceId (AUTO)
* FK ProductId + UserId
* StartDate
* EndDate (NULL)
* HostId (NULL)


## Use Cases

**Software Provider Sign Up**
 \<\<inlcude\>\>
 * Generate Keys
 * Compile (provider specific) license generator and libraries bundle.

**Software Provider Sign In**

**Create New License**

Alternative scenario : 
create new User, create new Product

**List Licences**

**(Renew License)**

**(Email Expiring Licenses)**