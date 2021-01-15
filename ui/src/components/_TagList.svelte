<script>
    export let taglist;

</script>

<ul class="list-group list-group-horizontal-sm">
    {#each taglist as tag}
        <li class="list-group-item" style="margin-right:5px;background-color:#ddd">{tag}</li>
    {/each}
</ul>
