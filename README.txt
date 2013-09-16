INTRODUCTION
------------

The GovDelivery TMS Integration module provides Drupal integration with the
GovDelivery Transactional Messaging Service (TMS). The module replaces the
backend SMTP library in your Drupal site with calls to the GovDelivery service,
so all mail sent from your site uses the ODM service.

Email configuration
-------------------

Setup settings.php with a default account name:

$govdelivery_account_map = array('default' => 'developer');
$conf['govdelivery_account_map'] = $govdelivery_account_map;

Then enter an email account in /admin/config/govdelivery/settings, with a
matching username to that in settings.php.

