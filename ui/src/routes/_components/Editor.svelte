<script>
    import "bytemd/dist/index.min.css";
    import { onMount } from "svelte";
    // import gfm from "@bytemd/plugin-gfm";

    let Editor = null;
    let plugins = null;
    export let value = "";

    onMount(async () => {
        const bytemd = await import("bytemd");
        Editor = bytemd.Editor;
    });

    function handleChange(e) {
        value = e.detail.value;
    }
</script>
<svelte:component this={Editor} on:change={handleChange} mode="tab" {value} />
