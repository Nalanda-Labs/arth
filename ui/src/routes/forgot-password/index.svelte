<script context="module">
	export async function preload({ params }, { user }) {
		if (user) {
			this.redirect(302, `/`);
		}
	}
	import { goto, stores } from "@sapper/app";
	import ListErrors from "../_components/ListErrors.svelte";
	import * as api from "api.js";

	let username = "";
	let email = "";
	let errors = null;
	let response = {};

	async function onSubmit(token) {
        if(username === "" && email === "") {
            alert("Please fill either email or username");
            return;
        }
		response = await api.post(`forgot-password`, {
			username,
			email,
		});

		// TODO handle network errors
		errors = response.error;

		if (errors) {
			alert(errors);
			return;
		} else {
            alert(response.message);
            goto('/')
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
				<h1 class="text-xs-center">Forgot Password</h1>
				<p class="text-xs-center">
                    <a href="/login">Have an account?</a>
                    or
                    <a href="/register">Sign up</a>
				</p>

				<ListErrors {response} />

				<form on:submit|preventDefault={onSubmit} id="r-form">
					<fieldset class="form-group">
						<label
							for="username"
							class="form-label">Username:</label>
						<input
							class="form-control form-control-sm"
							name="username"
							type="text"
							placeholder="username"
							bind:value={username} />
						<div class="form-text">Short without spaces</div>
					</fieldset>
					<fieldset class="form-group">
						<label for="email" class="form-label">Email:</label>
						<input
							class="form-control form-control-sm"
							type="email"
							placeholder="Email"
							bind:value={email} />
						<div class="form-text">
							We'll never share your email with anyone else.
						</div>
					</fieldset>
					<div class="wrapper">
						<button
							class="btn btn-lg btn-primary pull-xs-right">
							Forgot Password
						</button>
					</div>
				</form>
			</div>
		</div>
	</div>
</div>
