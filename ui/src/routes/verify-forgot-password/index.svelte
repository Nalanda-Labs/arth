<script context="module">
    export async function preload({ params }, { user }) {
        if (user) {
            this.redirect(302, `/`);
        }
    }
</script>

<script>
    import { goto } from "@sapper/app";
    import ListErrors from "../_components/ListErrors.svelte";
    import { onMount } from "svelte";
    import * as api from "api.js";

    let response = {};
    let new_password = "";
    let errors = "";

    async function onSubmit() {
        response = await api.post(`change-password?${window.location.search}`, {
            new_password
        });

        errors = response.error;

        if (errors) {
            alert(errors);
            return;
        } else {
            alert(response.message);
            goto("/");
        }
    }
</script>

<svelte:head>
    <title>Register • Arth</title>
</svelte:head>

<div class="auth-page">
    <div class="container page">
        <div class="row">
            <div class="col-md-6 offset-md-3 col-xs-12">
                <h1 class="text-xs-center">Change Password</h1>
                <ListErrors {response} />

                <form on:submit|preventDefault={onSubmit} id="r-form">
                    <fieldset class="form-group">
                        <label for="password" class="form-label"
                            >Passphrase:</label
                        >
                        <input
                            class="form-control form-control-sm"
                            type="password"
                            required
                            placeholder="Passphrase min 16 characters"
                            bind:value={new_password}
                            minlength="16"
                            maxlength="1024"
                        />
                        <div class="form-text">
                            Passphrase, longer than usual password. No
                            restriction. Never share.
                        </div>
                    </fieldset>
                    <div class="wrapper">
                        <button class="btn btn-lg btn-primary pull-xs-right">
                            Change Password
                        </button>
                    </div>
                </form>
            </div>
        </div>
    </div>
</div>

<style>
    .wrapper {
        width: 100%;
        height: 100px;
        display: flex;
        align-items: center;
        justify-content: center;
    }
</style>
