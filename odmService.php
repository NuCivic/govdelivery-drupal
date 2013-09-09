<?php

/**
 * @file
 * ODM Services for the GovDelivery Integration module.
 */

class ODMMessage {
  public $body;
  public $emailColumn;
  public $fromName;
  public $password;
  public $recordDesignator;
  public $subject;
  // ArrayOf_soapenc_string.
  public $to;
  public $userName;
}

class ReportingResponse {
  public $clickRate;
  public $clicks;
  public $clicksPCT;
  public $deferred;
  public $deferredPCT;
  public $deferredFailed;
  public $deferredSucceeded;
  public $deliveredPCT;
  public $endDate;
  public $failed;
  public $failedPCT;
  public $inQueue;
  public $invalid;
  public $lastClick;
  public $lastOpen;
  public $mph;
  public $opens;
  public $opensPCT;
  public $sent;
  public $sentPCT;
  public $serialID;
  public $startDate;
  public $status;
  public $total;
}


/**
 * ODMService class.
 */
class ODMService extends SoapClient {
  public $serverUri;

  protected static $classmap = array(
    'ODMMessage' => 'ODMMessage',
    'ReportingResponse' => 'ReportingResponse',
  );

  /**
   * Constructor.
   *
   * @param null $wsdl
   *   WSDL
   * @param array $options
   *   Options
   */
  public function __construct($wsdl, $options = array()) {
    foreach (self::$classmap as $key => $value) {
      if (!isset($options['classmap'][$key])) {
        $options['classmap'][$key] = $value;
      }
    }
    parent::__construct($wsdl, $options);
  }


  /**
   * Send message.
   *
   * @param ODMMessage $in0
   *   Message.
   *
   * @return ArrayOf_soapenc_string
   *   Array.
   */
  public function sendMessage(ODMMessage $in0) {
    try {
      $time_before = (timer_read('page') / 1000);
      watchdog("govdelivery", "About to call __soapCall - page timer: !timer", array('!timer' => $time_before), WATCHDOG_NOTICE);
      $result = $this->__soapCall('sendMessage', array($in0), array(
          'uri' => $this->serverUri,
          'soapaction' => '',
        )
      );
      $result_str = var_export($result, TRUE);
      $time_after = (timer_read('page') / 1000);
      watchdog("govdelivery", "Return from call to __soapCall - result: @result, page timer: @timer, elapsed time: @elapsed", array(
        '@result' => $result_str,
        '@timer' => $time_after,
        '@elapsed' => $time_after - $time_before,
      ), WATCHDOG_NOTICE);
      return $result;
    }
    catch (Exception $e) {
      watchdog("govdelivery", "Exception when calling GovDelivery SOAP Service: " . $e->getMessage());
      $result_str = var_export($result, TRUE);
      watchdog("govdelivery", "Exception in __soapCall - result: @result, page timer: @timer, elapsed time: @elapsed", array(
        '@result' => $result_str,
        '@timer' => $time_after,
        '@elapsed' => $time_after - $time_before,
      ), WATCHDOG_NOTICE);
      $ret = array();
      $ret[0] = 1;
      return $ret;
    }
  }


  /**
   * Report message.
   *
   * @param string $in0
   *   Message
   *
   * @return ReportingResponse
   *   Response.
   */
  public function messageReport($in0) {
    return $this->__soapCall('messageReport', array($in0), array(
        'uri' => $this->serverUri,
        'soapaction' => '',
      )
    );
  }
}
