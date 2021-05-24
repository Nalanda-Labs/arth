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
    import Swal from 'sweetalert2';

    export let id;
    export let slug;
    let reply_to_id;
    let user_replied;
    let image_url;
    let value = "";
    let Editor = null;
    let topic = {};
    let inProgress = false;
    let topics = [];

    const { session } = stores();

    function show_editor(reply_to, username) {
        reply_to_id = reply_to;
        user_replied = username;
        if (document.getElementById("editor").style.display === "none") {
            document.getElementById("editor").style.display = "block";
        } else {
            document.getElementById("editor").style.display = "none";
        }
        // TODO: Fix scroll on showing editor
        // var editorDiv = document.getElementById("container");
        // console.log(editorDiv.scrollHeight, editorDiv.scrollTop);
        // editorDiv.scrollTop = editorDiv.scrollHeight;
        // console.log(editorDiv.scrollHeight, editorDiv.scrollTop);
    }
    async function reply() {
        if ($session.user) {
            inProgress = true;
            if (value.length < 20 || value.length > 100000) {
                Swal.fire(
                    "Body should not be less than 20 or more than 100000 characters."
                );
                return;
            }

            let reply_to = reply_to_id;

            const response = await api.post(
                `create-post?topic_id=${id}`,
                { value, reply_to },
                localStorage.getItem("jwt")
            );

            if (response.post_id) {
                document.getElementById("editor").style.display = "none";
                const l = topics.length;
                topics[l] = {
                    topic_id: response.post_id,
                    description: value,
                    votes: 0,
                    posted_by: $session.user_id,
                    username: $session.user,
                    image_url: response.image_url,
                    shown_ts: "0 s",
                    initials: $session.user[0],
                };
            }
            inProgress = false;
        } else {
            Swal.fire("You are not logged in.");
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

<style>
    @media (max-width: 720px) {
		.topic {
			width: 100%;
		}
	}
	@media (max-width: 4096px) {
		.topic {
			width: 800px;
		}
    :global(.bytemd-editor .CodeMirror) {
        height: 90% !important;
    }
    .container {
        margin-left: 0px;
    }}
</style>

<div class="topic" id="container">
    <div class="row">
        <div class="col-xs-12">
            <Topics
                {id}
                {slug}
                bind:reply_to_id
                bind:topics
                bind:topic
                bind:user_replied
            />
            <div id="topics-end" style="display:none" />
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
                <!-- <a
                    href="/bookmark/{id}"
                    class="anchor"
                    title="Bookmark this post"
                    style="margin-right:5px"
                    ><span class="material-icons" style="vertical-align:bottom"
                        >bookmark</span
                    >Bookmark</a
                > -->
                <a
                    href="/reply"
                    on:click|preventDefault={show_editor(
                        reply_to_id,
                        user_replied
                    )}
                    class="anchor"
                    title="Reply to this post"
                    style="margin-right:5px"
                    ><span class="material-icons" style="vertical-align:bottom"
                        >reply</span
                    >Reply</a
                >
            {/if}
            <div id="editor" style="display:none;margin-top:10px">
                <span style="color:#4285F4">Replying to @{user_replied}</span>
                <svelte:component
                    this={Editor}
                    on:change={handleChange}
                    mode="tab"
                    {value}
                />
                <div class="b-wrapper">
                    <Button variant="raised">
                        <Label>Ask</Label>
                    </Button>
                </div>
            </div>
            <div style="min-height: 20px;"></div>
        </div>
    </div>
</div>
