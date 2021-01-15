<script>
    import "bytemd/dist/index.min.css";
    import { onMount } from "svelte";
    // import gfm from "@bytemd/plugin-gfm";

    export let body;
    let Editor = null;
    let plugins = null;

    onMount(async () => {
        const bytemd = await import("bytemd");
        Editor = bytemd.Editor;
    });

    function handleChange(e) {
        body = e.detail.value;
    }
</script>

<div class="editor-page">
    <hr />
    <fieldset class="form-group" style="margin-top:20px">
        <svelte:component this={Editor} on:change={handleChange} {body} />
    </fieldset>
</div>
