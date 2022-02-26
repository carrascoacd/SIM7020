# Arduino SIM7020 HTTP

Arduino HTTP library that works using the native HTTP client to provide the maximum performance and low memory footprint.

## How to install
The library is not published yet, you will need to download as ZIP and install it by hand.

## How to use

Do a POST request
```c
char response[64];
http.connect(APN);
http.post(HOST, PATH, "{\"action\": \"created\"}", response);
http.disconnect();
```

Do a GET request
```c
char response[64];
http.connect(APN);
http.get(HOST, PATH, response);
http.disconnect();
```

For more information check the `tests` folder where you will find all the tests writen with AUnit.

## Documentation

You will find a lot of documentation under the `doc` directory in case you need it for further development.

```
AT

OK
AT+CFUN=0

+CHTTPERR: 0,-1

+CPIN: NOT READY

OK
AT*MCGDEFCONT="IP","gprs-service.com"

OK
AT+CFUN=1

OK

+CPIN: READY
AT+CGATT?

+CGATT: 1

OK

AT+CGCONTRDP

+CGCONTRDP: 1,5,"gprs-service.com","10.160.18.24AT+CHTTPCREATE="https://xyz"

+CHTTPCREATE: 1

OK

AT+CHTTPCON=0

OK

AT+CHTTPSEND=0,1,"/api/devices/1/weather_entries",,"application/json",7B2277223A7B226D223A312C2274223A312C2268223A312C226D76223A312C227376223A312C227374223A317D7D

OK

+CHTTPNMIH: 0,200,309,Connection: keep-alive
Cache-Control: max-age=0, private, must-revalidate
Content-Length: 37
Content-Type: application/json; charset=utf-8
Date: Thu, 24 Feb 2022 19:44:39 GMT
Server: Cowboy
Strict-Transport-Security: max-age=31536000
X-Request-Id: ad2bbee7-e1c6-4c7f-8999-94af5ef24b44
Via: 1.1 vegur


+CHTTPNMIC: 0,0,23,23,7B22617070223A202248656C6C6F20576F726C6421227D

AT+CHTTPDISCON=0

OK
AT+CHTTPDESTROY=0

OK
```