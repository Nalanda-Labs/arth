<script context="module">
    export function preload({ params }, { user }) {
        let [id, slug] = [params.id, params.slug];
        return { id, slug };
    }
</script>

<script>
    // import Editor from "../../../_components/Editor.svelte";
    import Topics from "./_Topics.svelte";
    import { stores } from "@sapper/app";
    import Button, { Label } from "@smui/button";
    import { onMount } from "svelte";
    import * as api from "api.js";

    export let id;
    export let slug;
    let value = "";
    let reply_id = null;
    let Editor = null;

    const { session } = stores();

    function show_editor(topic_id) {
        reply_id = topic_id;
        if(document.getElementById("editor").style.display === "none") {
            document.getElementById("editor").style.display = "block";
        } else{
            document.getElementById("editor").style.display = "none";
        }
    }
    function reply() {
        console.log(value, reply_id);
        if($session.user) {
            
        }
    }
    onMount(async () => {
        const bytemd = await import("bytemd");
        Editor = bytemd.Editor;
    });

    function handleChange(e) {
        value = e.detail.value;
    }
</script>

<svelte:head>
    <title />
</svelte:head>

<div class="row">
    <div class="col-xs-12">
        <Topics {id} {slug} />
        <hr style="border-bottom:1px solidl;color:#eee" />
        {#if $session.user}
            <a
                href="/edit/{id}/{slug}"
                class="anchor"
                title="Edit your post"
                style="margin-right:5px"
                ><span class="material-icons" style="vertical-align:bottom"
                    >edit</span
                > Edit</a
            >
            <a
                href="/report/{id}"
                class="anchor danger"
                title="Report abusive or inappropriate content"
                style="margin-right:5px"
                ><span class="material-icons" style="vertical-align:bottom"
                    >report</span
                >Report</a
            >
            <a
                href="/share/{id}"
                class="anchor"
                title="Share a link to this post"
                style="margin-right:5px"
                ><span class="material-icons" style="vertical-align:bottom"
                    >share</span
                >Share</a
            >
            <a
                href="/bookmark/{id}"
                class="anchor"
                title="Bookmark this post"
                style="margin-right:5px"
                ><span class="material-icons" style="vertical-align:bottom"
                    >bookmark</span
                >Bookmark</a
            >
            <a
                href="/reply/{id}"
                on:click|preventDefault={show_editor(id)}
                class="anchor"
                title="Reply to this post"
                style="margin-right:5px"
                ><span class="material-icons" style="vertical-align:bottom"
                    >reply</span
                >Reply</a
            >
        {/if}
        <div id="editor" style="display:none;margin-top:10px">
            <svelte:component this={Editor} on:change={handleChange} mode="tab" {value} />
            <div style="margin-top:10px">
				<Button variant="raised" on:click="{reply}">
					<Label>Post</Label>
				</Button>
			</div>
        </div>
    </div>
</div>

<style>
    :global(.bytemd-editor .CodeMirror) {
        height: 90% !important;
    }
</style>
