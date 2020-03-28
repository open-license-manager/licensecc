#######################################
Extension points
#######################################

The version 2.0 of the library comes with clear API extension and customization points. 

Tweak hardware signature generator
***************************************

You've found the provided hardware signatures don't behave well for your customers (we can test on a limited set of 
hardware we've receiving reports of usage on environments we never imagined before). 
You can either change the way the default strategy behaves (choose for instance cpu type over disk label as default) or
provide your own identification method.

.. toctree::

   hardware_identifiers
   
Custom license locator 
***************************************
Your software struggle to find the license file ? you can implement your own way to find it. For instance if you want 
to download the license from a remote server, you could implement the logic here. 
 
.. TODO::
   
   this section need to be completed