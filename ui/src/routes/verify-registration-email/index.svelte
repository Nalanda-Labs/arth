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
    import * as api from "api.js";

    const { session } = stores();
    let response = {};

    onMount(async () => {
        response = await api.get(
            window.location.pathname + window.location.search
        );
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
