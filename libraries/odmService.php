<?php
/**
 * @file
 * ODM Services for the GovDelivery Integration module.
 */

/**
 * Base class for other classes representing XML data.
 */
class XMLData {
  /**
   * Replaces XML special characters.
   *
   * @param string $data
   *   XML data.
   *
   * @return string
   *   UTF8 XML string.
   */
  public function xmlSpecialChars($data) {
    // Strict checking is required for mb_detect to be useful.
    $strict = TRUE;
    // Empty string if it's not in the list of encodings to check.
    $is_encoded = mb_detect_encoding($data, array('UTF-8'), $strict);
    if (!empty($is_encoded)) {
      return preg_replace('@[\x00-\x08\x0B\x0C\x0E-\x1F]@', ' ', $data);
    }

    return utf8_encode(preg_replace('@[\x00-\x08\x0B\x0C\x0E-\x1F]@', ' ', $data));
  }
}

/**
 * Please use the constructor or the setters to prevent invalid characters
 * from making it into a SOAP request.
 */
class ODMCredentials extends XMLData {
  public $realm = 'ODM';
  public $username;
  public $password;

  /**
   * Constructor.
   *
   * @param string $username
   *   Username.
   * @param string $password
   *   Password.
   */
  public function __construct($username, $password) {
    $this->username = parent::xmlSpecialChars($username);
    $this->password = parent::xmlSpecialChars($password);
  }

  /**
   * Set the realm.
   *
   * @param string $data
   *   Realm data.
   */
  public function setRealm($data) {
    $this->realm = parent::xmlSpecialChars($data);
  }

  /**
   * Set username.
   *
   * @param string $username
   *   Username.
   */
  public function setUsername($username) {
    $this->username = parent::xmlSpecialChars($username);
  }

  /**
   * Set password.
   *
   * @param string $password
   *   Password.
   */
  public function setPassword($password) {
    $this->password = parent::xmlSpecialChars($password);
  }
}

/**
 * ODM message.
 */
class ODMMessage extends XMLData {
  public $body;
  public $emailColumn = 'email';
  public $fromName;
  public $recordDesignator = 'email';
  public $subject;
  public $to;
  public $trackClicks = TRUE;
  public $trackOpens = TRUE;


  /*public function __set($name, $value) {
  watchdog('ben', 'CALLED SET');
  $this->$name = parent::xmlSpecialChars($value);
  }*/

  /**
   * Set body.
   *
   * @param string $body
   *   Body.
   */
  public function setBody($body) {
    $this->body = parent::xmlSpecialChars($body);
  }

  /**
   * Set email column.
   *
   * @param string $email_column
   *   Email column.
   */
  public function setEmailColumn($email_column) {
    $this->emailColumn = parent::xmlSpecialChars($email_column);
  }

  /**
   * Set from name.
   *
   * @param string $from_name
   *   From name.
   */
  public function setFromName($from_name) {
    $this->fromName = parent::xmlSpecialChars($from_name);
  }

  /**
   * Set record designator.
   *
   * @param string $record_designator
   *   Record designator.
   */
  public function setRecordDesignator($record_designator) {
    $this->recordDesignator = parent::xmlSpecialChars($record_designator);
  }

  /**
   * Set subject.
   *
   * @param string $subject
   *   Subject.
   */
  public function setSubject($subject) {
    $this->subject = parent::xmlSpecialChars($subject);
  }
}

/**
 * Delivery activity.
 */
class ODMDeliveryActivitySince extends XMLData {
  public $maxresults;
  public $sequence = '';

  /**
   * Set sequence.
   *
   * @param string $sequence
   *   Sequence.
   */
  public function setSequence($sequence) {
    $this->sequence = parent::xmlSpecialChars($sequence);
  }
}

/**
 * ODMv2 Service class.
 */
class OdmService extends SoapClient {
  public $serverUri = 'http://odm.govdelivery.com/ODMv2';

  protected static $classmap = array(
    'credentials' => 'ODMCredentials',
    'message' => 'ODMMessage',
    'deliveryActivitySince' => 'ODMDeliveryActivitySince',
  );

  /**
   * Constructor.
   *
   * @param string $wsdl
   *   WDSL data.
   * @param ODMCredentials $credentials
   *   Credentials.
   * @param array $options
   *   Options.
   */
  public function __construct($wsdl, ODMCredentials $credentials, $options = array()) {
    foreach (self::$classmap as $key => $value) {
      if (!isset($options['classmap'][$key])) {
        $options['classmap'][$key] = $value;
      }
    }

    // @todo must be used if runtime debugging feature is added.
    $options['trace'] = TRUE;
    $this->setCredentialsHeader($credentials);

    parent::__construct($wsdl, $options);
  }

  /**
   * Set credentials header.
   *
   * @param ODMCredentials $credentials
   *   Credentials.
   */
  protected function setCredentialsHeader(ODMCredentials $credentials) {
    // Generate the needed credential headers.
    // @todo SimpleXMLElement::getDocNamespaces
    // Setup the correct namespace...I can't find a good way to query the
    // base namespace from the wsdl.
    // Encapsulating tag.
    // Generates an unencapsulated set of xml tags and their values.
    $header = new SoapHeader($this->serverUri,
                             'credentials',
                             new SoapVar($credentials, SOAP_ENC_OBJECT));

    // Set the credentials to the header section of the soap envelope.
    $this->__setSoapHeaders($header);
  }

  /**
   * Implements the wsdl sendMessage function.
   *
   * @todo Add a runtime debugging switch.
   *
   * @param ODMMessage $message
   *   The message.
   *
   * @return int
   *   Returns string on success and null or SoapFault exception on failure.
   */
  public function sendMessage(ODMMessage $message) {
    // Use the autogenerated base method that SoapClient class generates
    // when using a valid WSDL.
    $result = parent::sendMessage($message);
    return $result;
  }

  /**
   * Handle delivery activity.
   *
   * @param ODMDeliveryActivitySince $delivery_activity_since
   *   Activity data.
   */
  public function deliveryActivitySince(ODMDeliveryActivitySince $delivery_activity_since) {
    $result = parent::deliveryActivitySince($delivery_activity_since);

    /* $handle = fopen('/tmp/govd_odmv2.tmp', 'a+');
    fwrite($handle, 'Last Request' . PHP_EOL . $this->__getLastRequest());
    fclose($handle);
    */
    return $result;
  }
}
