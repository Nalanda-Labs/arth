<script context="module">
    export function preload({ params }, { user }) {
        let [id, slug] = params.id;
        return { id, slug };
    }
</script>

<script>
    import Editor from "../../../_components/Editor.svelte";
    import Topics from "./topics.svelte";
    import { goto, stores } from "@sapper/app";

    export let id;
    export let slug;

    const { session } = stores();
</script>

<svelte:head>
    <title />
</svelte:head>

<div class="editor-page">
    <div class="container page">
        <div class="row">
            <div class="col-md-8 offset-md-2 col-xs-12">
                <Topics {id} {slug} />
                {#if $session.user}
                    <Editor />
                {/if}
            </div>
        </div>
    </div>
</div>
