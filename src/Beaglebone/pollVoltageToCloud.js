var http = require('http');
var b = require('bonescript');  // running on Beaglebone
var inputPin = "P9_38";
var value = b.analogRead(inputPin);
var count=0;
var resetFlag=0;

callback = function(response) {  
	var str = '';  

	//another chunk of data has been recieved, so append it to`str`  
	response.on('data', function (chunk) {    
		str += chunk;  
	});  

	//the whole response has been recieved, so we just print itout here  
	response.on('end', function () {    
		console.log(str);    
		if (str=='reset') {count=0;resetFlag=1;}  
	});
}

process.on('uncaughtException', function(err) {  
	console.log('Caught exception: ' + err);
});

loop();

function loop() {    
	value = b.analogRead(inputPin);    
	var options = {
      	host: 'apps.panyuxin.com',       
   	path:'/test/plot/io.php?act=add&x='+count+'&y='+value*18.1+'&reset='+resetFlag,       
       	//if listening on a custom port, we need to specify it byhand       
       	//port: '1337',       
       	//headers: {'custom': 'Custom Header Demo works'}    
    };    
    resetFlag=0;    
    console.log(value);    
    http.request(options, callback).end();    
    count++;    
    setTimeout(loop, 1000);
}