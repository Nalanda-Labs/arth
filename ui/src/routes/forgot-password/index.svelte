<script context="module">
	export async function preload({ params }, { user }) {
		if (user) {
			this.redirect(302, `/`);
		}
	}
	import { goto, stores } from "@sapper/app";
	import ListErrors from "../_components/ListErrors.svelte";
	import * as api from "api.js";
	import Textfield from "@smui/textfield";
	import Icon from "@smui/textfield/icon/index";
	import Button, { Label } from "@smui/button";
	import HelperText from "@smui/textfield/helper-text/index";
	import "../_utils.scss";

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

<svelte:head>
	<title>Forgot Password • Arth</title>
</svelte:head>


<div class="wrapper">
	<div>
		<h3>Sign In</h3>
		<p>
			<a style="text-decoration:none" href="/register">Need an account?</a>
			or
			<a style="text-decoration:none" href="/login">Sign In</a>
			Provide either username or email.
		</p>

		<ListErrors {response} />

		<form on:submit|preventDefault={onSubmit}>
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
					bind:value={username}
					label="Username"
					type="text"
					minlength="16"
					maxlength="60"
					style="width:400px"
					input$aria-controls="helper-text-standard-c"
					input$aria-describedby="helper-text-standard-c">
				</Textfield>
				<HelperText id="helper-text-standard-c">
					Your username
				</HelperText>
			</div>
			<div class="b-wrapper">
				<Button variant="raised">
					<Label>Forgot Password</Label>
				</Button>
			</div>
		</form>
	</div>
</div>
