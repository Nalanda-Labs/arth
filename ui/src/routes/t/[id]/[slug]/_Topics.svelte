<script>
    import { onMount } from "svelte";
    import * as api from "api.js";
    import "bytemd/dist/index.min.css";
    import "../../../_utils.scss";
    import TagList from "../../../../components/_TagList.svelte";
    import Editor from "../../../_components/Editor.svelte";

    export let id;
    export let slug;
    let value = "";

    let topics = [];
    let title = "";
    let taglist = [];
    let Viewer = null;
    let offset = 0;
    let limit = 10;
    let count = 0;
    let time = "";
    let like_counter = 0;
    let reply_id = null;
    let bytemd = null;

    onMount(async () => {
        bytemd = await import("bytemd");
        Viewer = bytemd.Viewer;

        let response = await api.get(
            `t/${id}/${slug}`,
            localStorage.getItem("jwt")
        );

        if (response.topic) {
            title = response.title;
            value = response.topic.description;
            taglist = response.tags.map((tag) => tag.name);
            time = response.topic.created_at;
            like_counter = response.like_counter;
        }
        response = await api.get(
            `t/get-discussion/${id}/?time=${time}&limit=${limit}`,
            localStorage.getItem("jwt")
        );

        if (response.topics) {
            topics = response.topics.map((topic) => {
                topic.description, topic.id, topic.votes;
            });
            offset += limit;
            if (response.topics.length) {
                time = response.topics[response.topics.length - 1].created_at;
            }
        }
    });
    function show_editor(topic_id) {
        reply_id = topic_id;
        if(document.getElementById("editor").style.display === "none") {
            document.getElementById("editor").style.display = "block";
        } else{
            document.getElementById("editor").style.display = "none";
        }
    }
</script>

<svelte:head>
    <title>{title}</title>
</svelte:head>

<div>
    <h3>{title}</h3>
    <hr />
    <svelte:component this={Viewer} {value} />
    <TagList {taglist} />
    <div style="float:right">
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
    </div>
    {#each topics as { topic_id, desc, votes }}
        <hr style="border-bottom:1px solidl;color:#eee" />
        <svelte:component this={Viewer} {desc} />
        <div style="float:right">
            <a
                href="/edit/{topic_id}/{slug}"
                class="anchor"
                title="Edit your post"
                style="margin-right:5px"
                ><span class="material-icons" style="vertical-align:bottom"
                    >edit</span
                > Edit</a
            >
            <a
                href="/report/{topic_id}"
                class="anchor danger"
                title="Report abusive or inappropriate content"
                style="margin-right:5px"
                ><span class="material-icons" style="vertical-align:bottom"
                    >report</span
                >Report</a
            >
            <a
                href="/reply/{topic_id}"
                on:click|preventDefault={show_editor(topic_id)}
                class="anchor"
                title="Reply to this post"
                style="margin-right:5px"
                ><span class="material-icons" style="vertical-align:bottom"
                    >reply</span
                >Reply</a
            >
            <a
                href="/share/{topic_id}"
                class="anchor"
                title="Share a link to this post"
                style="margin-right:5px"
                ><span class="material-icons" style="vertical-align:bottom"
                    >share</span
                >Share</a
            >
            <a
                href="/bookmark/{topic_id}"
                class="anchor"
                title="Bookmark this post"
                style="margin-right:5px"
                ><span class="material-icons" style="vertical-align:bottom"
                    >bookmark</span
                >Bookmark</a
            >
        </div>
    {/each}
</div>
