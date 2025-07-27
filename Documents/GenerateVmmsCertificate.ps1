#
# PROJECT:    Mouri Internal Library Essentials
# FILE:       GenerateVmmsCertificate.ps1
# PURPOSE:    The script for generating a self-signed certificate for VMMS
#             service with 100 years validity.
# 
# LICENSE:    The MIT License
# 
# MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
#

# Reference: https://www.sysadmins.lv/blog-en/time-machine%C2%AE-for-microsoft-hyper-v-server-mythbuster-rebirth.aspx

#
# Generate a self-signed certificate for VMMS service with 100 years validity.
#

# Actual NotBefore (or Valid From) value is set to 1 day earlier. This is
# because by default CertEnroll interfaces generates a certificate that will
# balid only after 3-4 hours after certificate generation.
$NotBefore = (Get-Date).AddDays(-1)
$NotAfter = (Get-Date).AddYears(100)
try {$domain = ([System.DirectoryServices.ActiveDirectory.Domain]::GetCurrentDomain()).Name}
catch {}
if ($domain -eq $null) {$fqdn = $Env:COMPUTERNAME} else {$fqdn = $env:COMPUTERNAME + "." + $domain}
$SubjectDN = New-Object -ComObject X509Enrollment.CX500DistinguishedName
$SubjectDN.Encode("CN=$fqdn", 0x0)

$OID = New-Object -ComObject X509Enrollment.CObjectID
$OID.InitializeFromValue("1.3.6.1.5.5.7.3.1")
$OIDs = New-Object -ComObject X509Enrollment.CObjectIDs
$OIDs.Add($OID)
$EKU = New-Object -ComObject X509Enrollment.CX509ExtensionEnhancedKeyUsage
$EKU.InitializeEncode($OIDs)

$KU = New-Object -ComObject X509Enrollment.CX509ExtensionKeyUsage
$KU.InitializeEncode(0x30)
$KU.Critical = $false

# Here is a trick how to generate a custom certificate extension. Just
# instantiate IX%09Extension interface object and initialize it from
# the desired values.
$Unknown = New-Object -ComObject X509Enrollment.CX509Extension
$OID = New-Object -ComObject X509Enrollment.CObjectID
$OID.InitializeFromValue("1.3.6.1.4.1.311.62.1.1.1")
# AgEE represents a extension value in the Base64 form. Original value is a
# string of octets (02, 01, 04).
$Unknown.Initialize($OID,0x1,"AgEE")

$PrivateKey = New-Object -ComObject X509Enrollment.CX509PrivateKey
$PrivateKey.ProviderName = "Microsoft RSA SChannel Cryptographic Provider"
$PrivateKey.KeySpec = 0x1
# Here we set Key Encipherment and Data Encipherment key usage values.
$PrivateKey.KeyUsage = 0x30
$PrivateKey.Length = 2048
$PrivateKey.MachineContext = 0x1
# Since the certificate is used for testing purposes and not intended for
# productional environment we allow private key export.
$PrivateKey.ExportPolicy = 0x2
$PrivateKey.Create()

$Cert = New-Object -ComObject X509Enrollment.CX509CertificateRequestCertificate
$Cert.InitializeFromPrivateKey(0x2,$PrivateKey,"")
$Cert.Subject = $SubjectDN
$Cert.Issuer = $Cert.Subject
$Cert.NotBefore = $NotBefore
$Cert.NotAfter = $NotAfter
$Cert.X509Extensions.Add($EKU)
$Cert.X509Extensions.Add($KU)
$Cert.X509Extensions.Add($Unknown)
# Completing certificate request template building
$Cert.Encode()

$Request = New-Object -ComObject X509Enrollment.CX509enrollment
# Process request
$Request.InitializeFromRequest($Cert)
# Retrieve certificate encoded in Base64.
$endCert = $Request.CreateRequest(0x1)
$Cert = [Security.Cryptography.X509Certificates.X509Certificate2][Convert]::FromBase64String($endCert)
# Install certificate to user store
$Request.InstallResponse(0x2,$endCert,0x1,"")
$Cert = dir cert:\localmachine\my | ?{$_.Thumbprint -eq $Cert.Thumbprint}


#
# Configure private key access permissions
#
$PKPath = "$env:ALLUSERSPROFILE\Microsoft\Crypto\RSA\MachineKeys\$($Cert.PrivateKey.CspKeyContainerInfo.UniqueKeyContainerName)"
icacls $PKPath /grant "*S-1-5-83-0:(R)"
