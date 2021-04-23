<script context="module">
    export function preload({ params }, { user }) {
        let page = params.page;
        return { page };
    }
</script>

<script>
    import { stores } from "@sapper/app";
    import Button, { Label } from "@smui/button";
    import { onMount } from "svelte";
    import * as api from "api.js";

    let topics = [];
    export let page;

    onMount(async () => {
        if (!page) {
            page = 1;
        }

        let response = await api.get(
            `topics/${page}`,
            localStorage.getItem("jwt")
        );

        console.log(response)
    });
</script>

<div>
    <p>Hello</p>
</div>
