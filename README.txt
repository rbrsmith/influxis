INTRODUCTION
------------
The influxis module allows videos to be uploaded
to the influxis api using Drupal.
Videos are played back using flowplayer once 
they have completed the encoding process.

 * For a full description of the module, visit the project page:
   https://www.drupal.org/sandbox/rbrsmith/2404237

 * To submit bug reports and feature suggestions, or to track changes:
   https://www.drupal.org/project/issues/2404237

   
REQUIREMENTS
------------
This module requires the following modules:
 * Flowplayer5 (https://www.drupal.org/project/flowplayer5)
 
 Password encryption for influxis uses php mcrypt extension to be installed
 * Mcrypt (http://php.net/manual/en/book.mcrypt.php)  


 INSTALLATION
------------
 * Install as you would normally install a contributed Drupal module. See:
   https://drupal.org/documentation/install/modules-themes/modules-7
   for further information.


CONFIGURATION
-------------
 * Configure user permissions in Administration » People » Permissions:

   - Administer Influxis

     Users in roles with the "Administer Influxis" permission will see
     the the influxis configuration page under 
     Administration » Configuration » Media » Influxis .


 * Configure all the encoding settings sent in the request to influxis at 
 * Administration » Configuration » Media » Influxis
 * Username, password, API url and video url must be set.  
 * Video url is the http version of the RTMP/FMS url in 
 * account settings on influxis.com
  