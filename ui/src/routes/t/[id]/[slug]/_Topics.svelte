<script>
    import { onMount } from "svelte";
    import * as api from "api.js";
    import "bytemd/dist/index.min.css";
    import "../../../_utils.scss";
    import TagList from "../../../../components/_TagList.svelte";

    export let id;
    export let slug;

    let descs = [];
    let title = "";
    let taglist = [];
    let value = "";
    let Viewer = null;
    let offset = 0;
    let limit = 10;
    let count = 0;
    let time = "";

    onMount(async () => {
        const bytemd = await import("bytemd");
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
        }
        response = await api.get(
            `t/${id}/get-discussion/?time=${time}&limit=${limit}`,
            localStorage.getItem("jwt")
        );

        if (response.topics) {
            descs = response.topics.map(topic => topic.description);
            offset += limit;
            time = response.topics[response.topics.length - 1].created_at;
        }
    });    
</script>

<svelte:head>
    <title>{title}</title>
</svelte:head>

<div>
    <h3>{title}</h3>
    <hr />
    <svelte:component this={Viewer} {value} />
    <TagList {taglist} />
    <a href="/edit/{id}/{slug}" class="anchor" title="Edit your post"><span class="material-icons" style="vertical-align:bottom">edit</span> Edit</a>
    <a href="/report/{id}" class="anchor danger" title="Report abusive or inappropriate content"><span class="material-icons" style="vertical-align:bottom">report</span>Report</a>
    {#each descs as value}
    <hr/>
    <svelte:component this={Viewer} {value} />
    {/each}
</div>
