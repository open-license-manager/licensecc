# Issue licenses

The easiest way to issue licenses is to enter the project library and create the following project structure:
The projects folder can be anyware. We created one in `open-license-manager/projects` for your convenience (and for testing purposes). A default project<sup>1</sup> named `DEFAULT` has been created for you when you configured the project with cmake.
You can re-configure the project with the `LCC_PROJECT_NAME` cmake variable to create a new project.
 
```
projects
└── DEFAULT       #(your project name)
    ├── include
    │   └── licensecc
    │       └── DEFAULT
    │           ├── licensecc_properties.h
    │           └── public_key.h
    ├── licenses
    │   └── test.lic
    └── private_key.rsa
```

Let's suppose the `lcc` executable is in your path. If not you should find it in your build tree, or wherever you installed the library.

The lines below will create a perpetual unlimited license for your software:

```
cd projects/DEFAULT #(or whatever your project name is) 
lcc license issue -o licenses/{license-file-name}.lic
```

A good way to start exploring available options is the command:
`lcc license issue --help`

| Parameter        | Description                                                                                  |
|------------------|----------------------------------------------------------------------------------------------|
|base64,b          | the license is encoded for inclusion in environment variables                                |
|valid-from        | Specify the start of the validity for this license. Format YYYY-MM-DD. If not specified defaults to today. |
|valid-to          | The expire date for this license. Format YYYY-MM-DD. If not specified the license won't expire |
|client-signature  | The signature of the pc that requires the license. It should be in the format XXXX-XXXX-XXXX-XXXX. If not specified the license won't be linked to a specific pc. |
|output-file-name  | License output file path.                                                                    |
|extra-data        | Application specific data. They'll be returned when calling the `acquire_license` method   |
|feature-names     | Comma separated list of features to license. See `multi-feature` discussion.               |

Note:
<sup>1</sup> a project is a container for the customizations of open-license-manager. In special way its keys and build parameters. The name should reflect the name of the software you want to add a license to. The project name appears in the license file.