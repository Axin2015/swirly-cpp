var http = require('http');

http.get('http://localhost:8080/api/contr', function(res) {

    var body = '';

    res.on('data', function(chunk) {
        body += chunk;
    });

    res.on('end', function() {
        var data = JSON.parse(body);
        console.log(data);
    });

}).on('error', function(e) {
    console.log('error: ' + e.message);
});
