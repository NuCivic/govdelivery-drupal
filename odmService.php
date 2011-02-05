<?php
// $Id$

/**
 * @file
 * ODM Services for the GovDelivery Integration module.
 */

class ODMMessage {
  public $body; // string
  public $emailColumn; // string
  public $fromName; // string
  public $password; // string
  public $recordDesignator; // string
  public $subject; // string
  public $to; // ArrayOf_soapenc_string
  public $userName; // string
}

class ReportingResponse {
  public $clickRate; // string
  public $clicks; // string
  public $clicksPCT; // string
  public $deferred; // string
  public $deferredPCT; // string
  public $deferred_failed; // string
  public $deferred_succeeded; // string
  public $deliveredPCT; // string
  public $endDate; // string
  public $failed; // string
  public $failedPCT; // string
  public $inQueue; // string
  public $invalid; // string
  public $lastClick; // string
  public $lastOpen; // string
  public $mph; // string
  public $opens; // string
  public $opensPCT; // string
  public $sent; // string
  public $sentPCT; // string
  public $serialID; // string
  public $startDate; // string
  public $status; // string
  public $total; // string
}


/**
 * odmService class
 *
 *
 *
 * @author    {author}
 * @copyright {copyright}
 * @package   {package}
 */
class odmService extends SoapClient {
  public $server_uri;

  private static $classmap = array(
    'ODMMessage' => 'ODMMessage',
    'ReportingResponse' => 'ReportingResponse',
  );

  public function odmService($wsdl, $options = array()) {
    foreach (self::$classmap as $key => $value) {
      if (!isset($options['classmap'][$key])) {
        $options['classmap'][$key] = $value;
      }
    }
    parent::__construct($wsdl, $options);
  }

  /**
   *
   *
   * @param ODMMessage $in0
   * @return ArrayOf_soapenc_string
   */
  public function sendMessage(ODMMessage $in0) {
    try {
      $time_before = (timer_read('page') / 1000);
      watchdog("govdelivery", "About to call __soapCall - page timer: !timer", array('!timer' => $time_before ), WATCHDOG_NOTICE);
      $result = $this->__soapCall('sendMessage', array($in0), array(
          'uri' => $this->server_uri,
          'soapaction' => ''
        )
      );
      $result_str = var_export($result, TRUE);
      $time_after = (timer_read('page') / 1000);
      watchdog("govdelivery", "Return from call to __soapCall - result: @result, page timer: @timer, elapsed time: @elapsed", array('@result' => $result_str, '@timer' => $time_after, '@elapsed' => $time_after - $time_before ), WATCHDOG_NOTICE);
      return $result;
    } catch (Exception $e) {
        watchdog("govdelivery", "Exception when calling GovDelivery SOAP Service: " . $e->getMessage());
        $result_str = var_export($result, TRUE);
        watchdog("govdelivery", "Exception in __soapCall - result: @result, page timer: @timer, elapsed time: @elapsed", array('@result' => $result_str, '@timer' => $time_after, '@elapsed' => $time_after - $time_before ), WATCHDOG_NOTICE);
        $ret = array();
        $ret[0] = 1;
        return $ret;
    }
  }

  /**
   *
   *
   * @param string $in0
   * @return ReportingResponse
   */
  public function messageReport($in0) {
    return $this->__soapCall('messageReport', array($in0), array(
        'uri' => $this->server_uri,
        'soapaction' => ''
      )
    );
  }

}
