import * as api from 'api.js';
import jwt_decode from "jwt-decode";

export function post(req, res) {

    api.post('login', req.body).then(response => {
        let decoded = {};
        if (response.jwt) {
            decoded = jwt_decode(response.jwt);
        }

        if (decoded.username) { req.session.user = decoded.username; req.session.user_id = decoded.user_id}
        res.setHeader('Content-Type', 'application/json');

        res.end(JSON.stringify(response));
    });
}