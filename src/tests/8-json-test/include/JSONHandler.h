
#include <Arduino_JSON.h>

#define JSON_MIMETYPE "application/json"

//class JSONBody {
//public:
  //size_t     length;
  //JSONVar    *json;
  //const char _content;
  
  //JSONBody(
//};

//class JSONRequest: public AsyncWebServerRequest {
//private:
//public:
  //JSONVar json;
  
  //JSONRequest() : AsyncWebServerRequest() {}
//};

//typedef std::function<void(AsyncWebServerRequest *request, JSONVar &json)> JSONRequest;

class JSONHandler: public AsyncWebHandler {
private:
protected:
    String _uri;
    WebRequestMethodComposite _method;
    ArRequestHandlerFunction _onRequest;
    ArUploadHandlerFunction _onUpload;
    ArBodyHandlerFunction _onBody;
    size_t _contentLength;
    size_t _maxContentLength;
    const char *_content;
    bool _isRegex;
public:

  JSONHandler() 
  : _uri(), _method(HTTP_POST|HTTP_PUT|HTTP_PATCH), _onRequest(NULL), _onUpload(NULL), _onBody(NULL), _isRegex(false), _maxContentLength(1024) {}
  
  JSONHandler(const String& uri) 
  : _uri(uri), _method(HTTP_POST|HTTP_PUT|HTTP_PATCH), _onRequest(NULL), _onUpload(NULL), _onBody(NULL), _isRegex(false), _maxContentLength(1024) {}
  
  JSONHandler(const String& uri, ArRequestHandlerFunction onRequest) 
  : _uri(uri), _method(HTTP_POST|HTTP_PUT|HTTP_PATCH), _onRequest(onRequest), _onUpload(NULL), _onBody(NULL), _isRegex(false), _maxContentLength(1024) {}
  
  void setUri(const String& uri){ 
    _uri = uri; 
    _isRegex = uri.startsWith("^") && uri.endsWith("$");
  }
  void setMethod(WebRequestMethodComposite method){ _method = method; }
  void onRequest(ArRequestHandlerFunction fn){ _onRequest = fn; }
    
  virtual bool canHandle(AsyncWebServerRequest *request) override final{
    if(!_onRequest)
      return false;

    if(!(_method & request->method()))
      return false;

    
    if ( !request->contentType().equalsIgnoreCase(JSON_MIMETYPE) )
      return false;
    
    //todo: regex like "WebHandlerImpl.h" line 94
    if(_uri.length() && (_uri != request->url() && !request->url().startsWith(_uri+"/")))
      return false;

    request->addInterestingHeader("ANY");
    return true;
  }
  
  virtual void handleBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) override final {
    //Serial.println("handleBody");
    if (_onRequest) {
      _contentLength = total;
      if (total > 0 && request->_tempObject == NULL && total < _maxContentLength) {
        _content = (const char *) malloc(total);
      }
      if (_content != NULL) {
        Serial.println("[ERROR] '_content' malloc failed");
        memcpy((uint8_t*)(_content) + index, data, len);
      }
    }
  }
  
  virtual void handleRequest(AsyncWebServerRequest *request) override final {
    //Serial.println("handleRequest");
    if (!_onRequest) {
      request->send(501);
      return;
    }
    
    if (_content == NULL) {
      request->send(501);
      return;
    }
    
    JSONVar _json = JSON.parse(_content);
    
    if (JSON.typeof(_json) == "undefined") {
      Serial.println("[ERROR] JSON parsing failed!");
      request->send(400);
      return;
    }
    
    if (JSON.typeof(_json) == "null") {
      Serial.println("[ERROR] JSON null");
      request->send(400);
      return;
    }
    
    //Serial.print("printing json: ");
    //Serial.println(_json);
    
    request->_tempObject = &_json;
    
    _onRequest(request);
  }
};

