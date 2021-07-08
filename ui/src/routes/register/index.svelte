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
	import Swal from 'sweetalert2';
	import "../_utils.scss";
	import Snackbar, { Actions } from '@smui/snackbar';
	import IconButton from '@smui/icon-button';

	const { session } = stores();

	let username = "";
	let email = "";
	let password = "";
	let errors = null;
	let name = "";
	let token = {};
	let response = {};
	let errorWithClose;
	let messageWithClose;

	async function onSubmit(token) {
		response = await post(`auth/register`, {
			name,
			username,
			email,
			password,
			token,
		});

		if (response.error) {
			if(errors === 'User exists') {
				response.error = "Email already registered. If this is an error try restting your password or contact support.";
				errorWithClose.open();
			} else {
				errorWithClose.open();
			}
		} else {
			messageWithClose.open();
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
	<Snackbar bind:this={errorWithClose}>
		<Label>{response.error}</Label>
		<Actions>
		  <IconButton class="material-icons" title="Dismiss">close</IconButton>
		</Actions>
	</Snackbar>
	<Snackbar bind:this={messageWithClose}>
		<Label>{response.message}</Label>
		<Actions>
		  <IconButton class="material-icons" title="Dismiss">close</IconButton>
		</Actions>
	</Snackbar>
	<div>
		<h3>Sign Up</h3>
		<p class="text-xs-center"><a class="anchor" href="/login">Have an account?</a></p>
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
