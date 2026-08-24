#######################################
Extension points
#######################################

Version 2.0 of the library comes with defined customization points. 

API parameters, constants, defaults
***************************************

The main configuration point is the file `licensecc_properties.h` in the project directory. 
This contains the default buffer sizes used in the API, environment variable names, default strategies behavior. 

If one buffer isn't large enough have a look here. You may find his definition here.

This file is generated when the project is initialized (every time you generate a new project),
it is not under source control and it is for you to customize the library.


Tweak hardware signature generator
***************************************

If the provided hardware signatures don't behave well for your customers, or you want to change the default
way the library generates the pc identifier you can have a look at the following section.

First of all be sure to read about the standard behavior of :c:func:`identify_pc` here:

.. toctree::

   hardware_identifiers

   
Custom license locator 
***************************************
Your software struggle to find the license file? you can implement your own way to find it. For instance if you want 
to download the license from a remote server, you could implement the logic here. 
 
.. TODO::
   
   this section need to be completed
