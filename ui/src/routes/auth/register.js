import * as api from 'api.js';

export function post(req, res) {

	api.post('register', req.body ).then(response => {
		if (response.username) {
			req.session.user = response.username;
		}

		res.setHeader('Content-Type', 'application/json');

		res.end(JSON.stringify(response));
	});
}