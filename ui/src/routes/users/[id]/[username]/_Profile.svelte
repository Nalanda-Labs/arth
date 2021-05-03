<script context="module">
</script>

<script>
    import { goto, stores } from "@sapper/app";
    import { onMount } from "svelte";
    import * as api from "api.js";
    import jwt_decode from "jwt-decode";
    import Swal from "sweetalert2";
    import { mdiScrewLag } from "@mdi/js";

    export let username;
    export let id;
    let name = "";
    let title = "";
    let designation = "";
    let location = "";
    let image_url = "";
    let image_alt = "";
    let github = "";
    let website = "";
    let twitter = "";
    let response = {};

    let { session } = stores();

    onMount(async () => {
        response = await api.get(
            `user/${id}/${username}`,
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
            if (designation === "" && $session.user === response.username) {
                designation = "Click to edit your designation";
            }
            if (location === "" && $session.user === response.username) {
                location = "Click to edit your location";
            }
            if (github === "" && $session.user === response.username) {
                github = "Add your github url";
            }
            if (website === "" && $session.user === response.username) {
                website = "Add your website link";
            }
            if (twitter === "" && $session.user === response.username) {
                twitter = "Add your twitter profile";
            }
        }

        let username_elem = document.getElementById("username");

        username_elem.addEventListener(
            "blur",
            async function () {
                if (username != username_elem.innerHTML) {
                    let username1 = username_elem.innerHTML;
                    response = await api.post(
                        `profile/${id}/username/${username1}/`,
                        username1,
                        localStorage.getItem("jwt")
                    );
                    if (response.error) {
                        Swal.fire(response.error);
                    } else if (response.jwt) {
                        Swal.fire("You will be logged out for username change");
                        await api.post(`auth/logout`);
                        $session.user = null;
                        localStorage.removeItem("jwt");
                        goto("/login");
                    }
                }
            },
            false
        );

        let title_elem = document.getElementById("title");

        title_elem.addEventListener(
            "blur",
            async function () {
                if (title != title_elem.innerHTML) {
                    let title1 = title_elem.innerHTML;

                    response = await api.post(
                        `profile/${id}/title/${title1}/`,
                        title1.trim(),
                        localStorage.getItem("jwt")
                    );
                    if (response.error) {
                        Swal.fire(response.error);
                    } else {
                        title = title_elem.innerHTML;
                    }
                }
            },
            false
        );

        let name_elem = document.getElementById("name");

        name_elem.addEventListener(
            "blur",
            async function () {
                if (name != name_elem.innerHTML) {
                    let name1 = name_elem.innerHTML;

                    response = await api.post(
                        `profile/${id}/name/${name1}/`,
                        name1.trim(),
                        localStorage.getItem("jwt")
                    );
                    if (response.error) {
                        Swal.fire(response.error);
                    } else {
                        name = name_elem.innerHTML;
                    }
                }
            },
            false
        );

        let d_elem = document.getElementById("designation");

        d_elem.addEventListener(
            "blur",
            async function () {
                if (designation != d_elem.innerHTML) {
                    let d1 = d_elem.innerHTML;

                    response = await api.post(
                        `profile/${id}/designation/${d1}/`,
                        d1.trim(),
                        localStorage.getItem("jwt")
                    );
                    if (response.error) {
                        Swal.fire(response.error);
                    } else {
                        designation = d_elem.innerHTML;
                    }
                }
            },
            false
        );

        let location_elem = document.getElementById("location");

        location_elem.addEventListener(
            "blur",
            async function () {
                if (location != location_elem.innerHTML) {
                    let location1 = location_elem.innerHTML;

                    response = await api.post(
                        `profile/${id}/location/${d1}/`,
                        location1.trim(),
                        localStorage.getItem("jwt")
                    );
                    if (response.error) {
                        Swal.fire(response.error);
                    } else {
                        location = location_elem.innerHTML;
                    }
                }
            },
            false
        );
    });
</script>

<div class="row" style="margin-top:10px">
    <div class="col-12 col-sm-12 col-md-2" style="float:left;margin-right:10px">
        <img
            src="{image_url}?s=160"
            alt="{username}'s proile image"
            width="160px"
        />
        <p>
            Your gravatar, <a href="https://en.gravatar.com/site/signup/"
                >update gravatar</a
            > to change it.
        </p>
    </div>
    <div class="col-12 col-sm-12 col-md-6" style="float:left">
        <table>
            <tr>
                {#if $session.user == username}
                    <td
                        ><span
                            contenteditable="true"
                            id="username"
                            style="font-size:20px;font-weight: 500;"
                            title="Click to edit">{username}</span
                        ></td
                    >
                {:else}
                    <td><span id="username">{username}</span></td>
                {/if}
            </tr>
            <tr>
                {#if $session.user == username}
                    <td
                        ><span
                            contenteditable="true"
                            id="title"
                            title="Click to edit">{title}</span
                        ></td
                    >
                {:else}
                    <td><span id="title">{title}</span></td>
                {/if}
            </tr>
            <tr>
                {#if $session.user == username}
                    <td><span contenteditable="true" id="name">{name}</span></td
                    >
                {:else}
                    <td><span id="name">{name}</span></td>
                {/if}
            </tr>
            <tr>
                {#if $session.user == username}
                    <td
                        ><span contenteditable="true" id="designation"
                            >{designation}</span
                        ></td
                    >
                {:else}
                    <td><span id="designation">{designation}</span></td>
                {/if}
            </tr>
        </table>
    </div>
    <div class="col-12 col-sm-12 col-md-3" style="display:inline">
        <table>
            <tr>
                {#if $session.user == username}
                    <td>
                        <i class="fas fa-map-marker-alt" style="color:#666" />
                        <span contenteditable="true" id="location">
                            {location}
                        </span>
                    </td>
                {:else}
                    <td>
                        <i class="fas map-marker-alt" style="color:#666" />
                        <span id="location">
                            {location}
                        </span>
                    </td>
                {/if}
            </tr>
            <tr>
                {#if $session.user == username}
                    <td>
                        <i class="fab fa-github" style="color:#666" />
                        <span contenteditable="true" id="github">
                            {github}
                        </span>
                    </td>
                {:else}
                    <td>
                        <i class="fab fa-github" style="color:#666" />
                        <span id="location">
                            {github}
                        </span>
                    </td>
                {/if}
            </tr>
            <tr>
                {#if $session.user == username}
                    <td>
                        <i class="fas fa-link" style="color:#666" />
                        <span contenteditable="true" id="website">
                            {website}
                        </span>
                    </td>
                {:else}
                    <td>
                        <i class="fas fa-link" style="color:#666" />
                        <span id="website">
                            {website}
                        </span>
                    </td>
                {/if}
            </tr>
            <tr>
                {#if $session.user == username}
                    <td>
                        <i class="fab fa-twitter" style="color:#666" />
                        <span contenteditable="true" id="twitter">
                            {twitter}
                        </span>
                    </td>
                {:else}
                    <td>
                        <i class="fab fa-twitter" style="color:#666" />
                        <span id="twitter">
                            {twitter}
                        </span>
                    </td>
                {/if}
            </tr>
        </table>
    </div>
</div>
