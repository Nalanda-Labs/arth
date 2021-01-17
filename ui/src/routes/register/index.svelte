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
	import { recaptchaSiteKey } from "../../consts.js";
	import { onMount } from "svelte";
	import Textfield from "@smui/textfield";
	import Icon from "@smui/textfield/icon/index";
	import HelperText from "@smui/textfield/helper-text/index";
	import Button, { Label } from "@smui/button";
	import "../_utils.scss";

	const { session } = stores();

	let username = "";
	let email = "";
	let password = "";
	let errors = null;
	let name = "";
	let token = {};
	let response = {};

	async function onSubmit(token) {
		response = await post(`auth/register`, {
			name,
			username,
			email,
			password,
			token,
		});

		// TODO handle network errors
		errors = response.error;

		if (errors) {
			return;
		} else {
		}
	}

	onMount(() => {
		if ($session.user) {
			goto("/");
			return;
		}
		const script = document.createElement("script");
		script.type = "text/javascript";
		script.src = "https://www.google.com/recaptcha/api.js?&render=explicit";
		document.getElementsByTagName("head")[0].appendChild(script);
	});
</script>

<svelte:head>
	<title>Register • Arth</title>
</svelte:head>

<div class="wrapper">
	<ListErrors {response} />
	<div>
		<h3>Sign Up</h3>
		<p class="text-xs-center"><a href="/login">Have an account?</a></p>
		<form on:submit|preventDefault={onSubmit}>
			<div>
				<Textfield
					withTrailingIcon
					bind:value={username}
					label="Username"
					type="text"
					minlength="3"
					maxlength="32"
					style="width:400px"
					input$aria-controls="helper-text-standard-c"
					input$aria-describedby="helper-text-standard-c">
					<Icon class="material-icons">person</Icon>
				</Textfield>
				<HelperText id="helper-text-standard-c">
					Short without spaces
				</HelperText>
			</div>
			<div>
				<Textfield
					withTrailingIcon
					bind:value={name}
					label="Name"
					type="text"
					minlength="3"
					maxlength="128"
					style="width:400px"
					input$aria-controls="helper-text-standard-c"
					input$aria-describedby="helper-text-standard-c">
					<Icon class="material-icons">name</Icon>
				</Textfield>
				<HelperText id="helper-text-standard-c">
					Your real name
				</HelperText>
			</div>
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
				<Button
					class="g-recaptcha"
					data-sitekey={recaptchaSiteKey}
					data-callback="onSubmit"
					data-action="submit"
					variant="raised">
					<Label>Sign Up</Label>
				</Button>
			</div>
		</form>
	</div>
</div>
