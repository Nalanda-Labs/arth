<script context="module">
</script>

<script>
    import { stores } from "@sapper/app";
    import { onMount } from "svelte";
    import * as api from "api.js";

    export let username;
    let name = "No Name";
    let title = "No title";
    let designation = "";
    let location = "";
    let image_url = "";
    let image_alt = "";
    let response = {};

    let { session } = stores();

    onMount(async () => {
        response = await api.get(
            `profile/${username}`,
            localStorage.getItem("jwt")
        );

        if (!response.error) {
            name = response.name;
            title = response.title;
            designation = response.designation;
            location = response.location;
            username = response.username;

            if (response.image_url !== "") {
                image_url = response.image_url;
                image_alt = name;
            } else {
                image_alt = "No profile photo";
            }
            if (title === "" && $session.user === response.username) {
                title = "Click to edit your title";
            }
            if (name === "" && $session.user === response.username) {
                name = "Click to edit your name";
            }
        }
    });
</script>

<div class="row">
    <div class="col-12 col-sm-12 col-md-2" style="float:left;margin-right:10px">
        <img src={image_url} alt={image_alt} width="160px" height="200px" />
    </div>
    <div class="col-12 col-sm-12 col-md-6" style="float:left">
        <table>
            <tr>
                <td style="float:right;margin-right:10px">Title:</td>
                {#if $session.user == username}
                    <td><span contenteditable="true">{title}</span></td>
                {:else}
                    <td><span>{title}</span></td>
                {/if}
            </tr>
            <tr>
                <td style="float:right;margin-right:10px">Name:</td>
                {#if $session.user == username}
                    <td><span contenteditable="true">{name}</span></td>
                {:else}
                    <td><span>{name}</span></td>
                {/if}
            </tr>
        </table>
    </div>
    <div class="col-12 col-sm-12 col-md-3" style="display:inline">
        <p class="card-text">It is software - made up of bugs. Col3</p>
    </div>
</div>
