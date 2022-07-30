const express = require('express');
const path = require('path');
const app = express();

var child_process = require('child_process');

// Set directory to contain the templates ('views')
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'ejs');

const port = 3000;

app.get('/', function(req, res) {
  res.render('basic.ejs');
});

app.post('/', function(req, res) {
    child_process.exec('./gitathome', function (err, stdout, stderr){
        if (err) {
            console.log("child processes failed with error code: " +
                err.code);
        }
        res.send(stdout);
    });
});


app.listen(port, function() {
  console.log(`Example app listening on port ${port}!`)
});
