export function post(req, res) {
	delete req.session.user;
	delete req.session.username;
	res.end(JSON.stringify({ ok: true }));
}