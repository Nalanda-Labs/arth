import * as api from 'api.js';

export function post(req, res) {
	const user = req.body;

	api.post('register', { user }).then(response => {
		if (response.username) {
			req.session.user = response.username;
		}

		res.setHeader('Content-Type', 'application/json');

		res.end(JSON.stringify(response));
	});
}