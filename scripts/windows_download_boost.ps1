#!/usr/bin/env pwsh

# This script downloads boost. It must be invoked with 4 arguments
# windows_download_boost.ps1 <boost_version> <msvc version> <architecture> <output_dir>
# eg. windows_download_boost.ps1 1.91.0 14.3 64 C:\local\boost

if ($args.Count -ne 4) {
    Write-Output "Error: This script requires 4 parameters.\n windows_download_boost.ps1 <boost_version> <msvc version> <architecture> <output_dir>"
    exit
}
#set Invoke-WebRequest to not show progress bar, as it can cause issues in some CI environments
$ProgressPreference = 'SilentlyContinue'

[string] $version = $args[0]
[string] $msvc_version = $args[1]
[string] $architecture = $args[2]
[string] $output_dir = $args[3]

[string] $outputfile = Join-Path -Path $output_dir -ChildPath "boost.exe"
[int]$maxRetries = 3
[int]$retryCount = 0
[int]$StatusCode = 200


$version_und = $version.Replace('.', '_')
$uri = "https://github.com/userdocs/boost/releases/download/boost-"+$version+"/boost_" + $version_und + "-msvc-" + $msvc_version + "-" + $architecture + ".exe"

if (-not (Test-Path $output_dir)) {
    New-Item -ItemType Directory -Path $output_dir
}

if (-not (Test-Path $outputfile)) {
	Write-Verbose "Boost not cached, downloading it: from $uri to $outputfile"
    do {
        try {
                Invoke-WebRequest -Uri "$uri" -Verbose -OutFile "$outputfile" 
                break
            } catch {
                $StatusCode = $_.Exception.Response.StatusCode
                $errorMessage = $_.Exception.Message
                $retryCount++
                Write-Output "Attempt $retryCount failed: $StatusCode $errorMessage. Retrying $uri ..."
                Start-Sleep -Seconds 2  # Wait before retrying
            }
        } until ($retryCount -ge $maxRetries)

        if ($retryCount -ge $maxRetries) {
            Write-Output "Request failed after $retryCount attempts."
            exit 1
        } else {
            Write-Output "Boost downloaded"
            dir $output_file
        }
} else { 
    Write-Output "Boost already downloaded" 
    dir $output_file
}
