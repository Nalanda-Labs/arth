<script context="module">
    export function preload({ params }, { user }) {
        let [id, slug] = [params.id, params.slug];
        return { id, slug };
    }
</script>

<script>
    import Editor from "../../../_components/Editor.svelte";
    import Topics from "./_Topics.svelte";
    import { stores } from "@sapper/app";

    export let id;
    export let slug;
    let body="";

    const { session } = stores();
</script>

<svelte:head>
    <title />
</svelte:head>

<div class="editor-page">
    <div class="container-fluid page">
        <div class="row">
            <div class="col-md-8 offset-md-2 col-xs-12">
                <Topics {id} {slug} />
                <h4>Post your ideas</h4>
                <hr/>
                {#if $session.user}
                    <Editor mode="tab" {body}/>
                {/if}
            </div>
        </div>
    </div>
</div>
