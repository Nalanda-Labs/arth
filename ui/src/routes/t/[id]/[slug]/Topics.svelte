<script>
    import { onMount } from "svelte";
    import * as api from "api.js";
    import "bytemd/dist/index.min.css";
    import TagList from "../../../../components/_TagList.svelte";

    export let id;
    export let slug;

    let topics = [];
    let title = "";
    let taglist = [];
    let value = "";
    let Viewer = null;

        onMount(async () => {
        const bytemd = await import("bytemd");
        Viewer = bytemd.Viewer;

        const response = await api.get(
            `t/${id}/${slug}`,
            localStorage.getItem("jwt")
        );

        console.log(response);

        if (response.topic) {
            topics = response.topic;
            title = response.title;            
            value = topics[0].description;
            taglist = response.tags.map(tag => tag.name);
        }
    });
</script>

<div>
    <h3>{title}</h3>
    <hr/>
    <svelte:component this={Viewer} {value} />
    <TagList {taglist} />
</div>
