<script context="module">
	export async function preload({ params }, { user }) {
		if (user) {
			this.redirect(302, `/`);
		}
	}
</script>

<script>
	import { goto, stores } from "@sapper/app";
	import ListErrors from "../_components/ListErrors.svelte";
	import { post } from "utils.js";
	import jwt_decode from "jwt-decode";
	import Textfield from "@smui/textfield";
	import Icon from "@smui/textfield/icon/index";
	import Button, { Label } from "@smui/button";
	import HelperText from "@smui/textfield/helper-text/index";
	import Swal from 'sweetalert2';
	import "../_utils.scss";

	const { session } = stores();

	let email = "";
	let password = "";
	let errors = null;
	let response = {};

	async function submit(event) {
		const response = await post(`auth/login`, { email, password });

		if (response.error) {
			Swal.fire(response.error);
		} else if (response.jwt) {
			const decoded = jwt_decode(response.jwt);
			localStorage.setItem("jwt", response.jwt);
			$session.user = decoded["username"];
			$session.user_id = decoded["user_id"];
			goto("/");
		}
	}
</script>

<svelte:head>
	<title>Sign in • Arth</title>
</svelte:head>

<div class="wrapper">
	<div>
		<h3>Sign In</h3>
		<p>
			<a class="anchor" style="text-decoration:none" href="/register">Need an account?</a>
			or
			<a class="anchor" style="text-decoration:none" href="/forgot-password">Forgot
				password?</a>
		</p>

		<ListErrors {response} />

		<form on:submit|preventDefault={submit}>
			<div>
				<Textfield
					withTrailingIcon
					bind:value={email}
					label="Email"
					type="email"
					minlength="6"
					maxlength="256"
					style="width:400px"
					input$aria-controls="helper-text-standard-c"
					input$aria-describedby="helper-text-standard-c">
					<Icon class="material-icons">email</Icon>
				</Textfield>
				<HelperText id="helper-text-standard-c">
					We will never share your email with anyone
				</HelperText>
			</div>
			<div>
				<Textfield
					withTrailingIcon
					bind:value={password}
					label="Password"
					type="password"
					minlength="16"
					maxlength="60"
					style="width:400px"
					input$aria-controls="helper-text-standard-c"
					input$aria-describedby="helper-text-standard-c">
					<Icon class="material-icons">password</Icon>
				</Textfield>
				<HelperText id="helper-text-standard-c">
					16 characters. Never share.
				</HelperText>
			</div>
			<div class="b-wrapper">
				<Button variant="raised">
					<Label>Log In</Label>
				</Button>
			</div>
		</form>
	</div>
</div>
