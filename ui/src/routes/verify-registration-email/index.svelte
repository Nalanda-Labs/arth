<script context="module">
    export async function preload({ params }, { user }) {
        if (user) {
            this.redirect(302, `/`);
        }
    }
</script>

<script>
    import { goto, stores } from "@sapper/app";
    import { onMount } from "svelte";
    import * as api from "api.js";
    import Snackbar, { Actions } from '@smui/snackbar';
    import IconButton from '@smui/icon-button';
    import Button, { Label } from "@smui/button";


    const { session } = stores();
    let response = {};
    let errorWithClose;
    let messageWithClose;

    onMount(async () => {
        response = await api.get(
            window.location.pathname + window.location.search
        );

	if (response.error) {
		errorWithClose.open();
	} else {
		messageWithClose.open();
	}
    });
</script>

<svelte:head>
    <title>Register • Arth</title>
</svelte:head>

<div class="auth-page">
    <div class="container page">
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
    </div>
</div>
