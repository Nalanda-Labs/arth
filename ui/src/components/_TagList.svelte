<script>
    import "../routes/_utils.scss";
    export let taglist;
</script>

<ul style="list-style-type:none;padding:0px;display:inline">
    {#each taglist as tag}
        <li style="margin-right:5px;background-color:#ddd;display:inline;padding:7px"><a href="/tags/{tag}" class="anchor">{tag}</a></li>
    {/each}
</ul>
