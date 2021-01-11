<script context="module">
	export async function preload({ params }, { user }) {
		if (user) {
			this.redirect(302, `/`);
		}
	}
</script>

<script>
	import { goto, stores } from '@sapper/app';
	import ListErrors from '../_components/ListErrors.svelte';
	import { post } from 'utils.js';
	import jwt_decode from "jwt-decode";

	const { session } = stores();

	let email = '';
	let password = '';
	let errors = null;
	let response = {};

	async function submit(event) {
		const response = await post(`auth/login`, { email, password });

		if (response.error) {
			alert(response.error);
		}else if (response.jwt) {
			const decoded = jwt_decode(response.jwt);
			localStorage.setItem("jwt", response.jwt);
			$session.user = decoded["username"];
			goto('/');
		}
	}
</script>
<style>
	.wrapper {
		width: 100%;
		height: 100px;
		display: flex;
		align-items: center;
		justify-content: center;
	}
</style>

<svelte:head>
	<title>Sign in • Arth</title>
</svelte:head>

<div class="auth-page">
	<div class="container page">
		<div class="row">
			<div class="col-md-6 offset-md-3 col-xs-12">
				<h1 class="text-xs-center">Sign In</h1>
				<p class="text-xs-center">
					<a href="/register">Need an account?</a>
				</p>

				<ListErrors {response}/>

				<form on:submit|preventDefault={submit}>
					<fieldset class="form-group">
						<label for="email" class="form-label">Email:</label>
						<input class="form-control form-control-sm" type="email" required placeholder="Email" bind:value={email}>
						<div class="form-text">We'll never share your email with anyone else.</div>
					</fieldset>
					<fieldset class="form-group">
						<label for="password" class="form-label">Passphrase:</label>
						<input class="form-control form-control-sm" type="password" required placeholder="Passphrase min 16 characters" bind:value={password} minlength="16" maxlength="1024">
						<div class="form-text">Never share.</div>
					</fieldset>
					<div class="wrapper">
						<button class="btn btn-lg btn-primary pull-xs-right" style="">
							Sign In
						</button>
					</div>
				</form>
			</div>
		</div>
	</div>
</div>