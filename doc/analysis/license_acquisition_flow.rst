########################
License acquisition flow
########################

Short description of the architecture used by ``LicenseFacade::acquire_license``
after the cursor/parser refactor.

Components
**********

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - Component
     - Responsibility
   * - ``LicenseFacade``
     - Orchestrates the whole flow; owns the ``EventRegistry`` and the
       ``LicenseVerifier``.
   * - ``LocatorFactory::get_active_strategies``
     - Builds the list of active ``LocatorStrategy`` objects (application folder,
       env vars, caller-provided location, extra strategies).
   * - ``FoundLicenseCursor``
     - Iterates over the strategies and their locations, yielding ``RawLicenseData``
       (location id + raw license content). Strategies that return no locations
       are skipped.
   * - ``LicenseParser::parseLicense``
     - Parses one ``RawLicenseData`` (INI) into zero or more ``FullLicenseInfo``.
   * - ``LicenseVerifier::verify_license``
     - Verifies (signature and limits) one ``FullLicenseInfo``, returns
       ``LicenseInfoEx`` (``LicenseInfo`` + ``FUNCTION_RETURN``).
   * - ``LicenseFacade::mergeLicenses``
     - Picks the best valid license (no-input, or latest expiry) and fills
       ``LicenseInfo``; decides the final ``LCC_EVENT_TYPE``.

Sequence diagram
****************

.. mermaid::

   sequenceDiagram
      participant C as C API
      participant F as LicenseFacade
      participant LF as LocatorFactory
      participant P as LicenseParser
      participant V as LicenseVerifier

      C->>+F: acquire_license(callerInfo, <br> licenseLocation, license_out)
      F->>LF: get_active_strategies(strategies, licenseLocation)
      LF-->>F: vector <LocatorStrategy>
 
      loop each strategy and location (FoundLicenseCursor) 
          F->>+P: parseLicense(RawLicenseData)
          P-->>-F: vector <FullLicenseInfo>
          loop each FullLicenseInfo
              F->>+V: verify_license(fullLicenseInfo)
              V-->>-F: LicenseInfoEx (return_code + LicenseInfo)
          end
      end
      
      F->>F: mergeLicenses(all_results, er, license_out)
      F-->>-C: result

Notes
*****

- Strategies that return no locations are skipped by the cursor.
- Events (found/not found/malformed) are recorded in the shared
  ``EventRegistry`` and exported to ``LicenseInfo::status`` at the end.
- ``mergeLicenses`` chooses the license with no expiry, else the one with the
  latest ``days_left``/expiry among the valid ones.

