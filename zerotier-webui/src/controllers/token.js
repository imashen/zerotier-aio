// Desc: Token controller

const fs = require('fs');
const util = require('util');

const readFile = util.promisify(fs.readFile);

let _token = process.env.ZT_TOKEN;

exports.get = async function() {
  if (_token) {
    return _token;
  } else {
    try {
      _token = await readFile('/var/lib/zerotier-one/authtoken.secret', 'utf8');
      return _token;
    } catch(err) {
      throw(err);
    }
  }
}
