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

	const { session } = stores();

	let username = '';
	let email = '';
	let password = '';
	let errors = null;
	let name='';

	async function submit(event) {
		const response = await post(`/api/v1/register`, { name, username, email, password });

		// TODO handle network errors
		errors = response.error;

		if(errors) {
			alet(error);
			return;
		}

		if (response.username) {
			$session.user = response.username;
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
	<title>Register • Arth</title>
</svelte:head>

<div class="auth-page">
	<div class="container page">
		<div class="row">
			<div class="col-md-6 offset-md-3 col-xs-12">
				<h1 class="text-xs-center">Sign up</h1>
				<p class="text-xs-center">
					<a href="/login">Have an account?</a>
				</p>

				<ListErrors {errors}/>

				<form on:submit|preventDefault={submit}>
					<fieldset class="form-group">
						<label for="useename" class="form-label">Username:</label>
						<input class="form-control form-control-sm" name="username" type="text" required placeholder="username"  bind:value={username}>
						<div class="form-text">Short without spaces</div>
					</fieldset>
					<fieldset class="form-group">
						<label for="name" class="form-label">Name:</label>
						<input class="form-control form-control-sm" name="name" type="text" required placeholder="Your name" bind:value={name} minlength="3">
						<div class="form-text">You know, your real name.</div>
					</fieldset>
					<fieldset class="form-group">
						<label for="email" class="form-label">Email:</label>
						<input class="form-control form-control-sm" type="email" required placeholder="Email" bind:value={email}>
						<div class="form-text">We'll never share your email with anyone else.</div>
					</fieldset>
					<fieldset class="form-group">
						<label for="password" class="form-label">Passphrase:</label>
						<input class="form-control form-control-sm" type="password" required placeholder="Passphrase min 16 characters" bind:value={password} minlength="16" maxlength="1024">
						<div class="form-text">Passphrase, longer than usual password. No restriction. Never share.</div>
					</fieldset>
					<div class="wrapper">
						<button class="btn btn-lg btn-primary pull-xs-right" style="">
							Sign up
						</button>
					</div>
				</form>
			</div>
		</div>
	</div>
</div>