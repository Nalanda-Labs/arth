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
	import { onMount } from "svelte";

    const { session } = stores();
    let response = {}

	onMount(() => {
        response = fetch(`/api/v1` + window.location.pathname + window.location.search);
        if(response.error) {
            alert(error);
            return;
        }

        alert(response.message);
        goto('/');
	});
</script>

<svelte:head>
	<title>Register • Arth</title>
</svelte:head>

<div class="auth-page">
	<div class="container page">
		<div class="row">
			<div class="col-md-6 offset-md-3 col-xs-12">
				<ListErrors {response} />
			</div>
		</div>
	</div>
</div>
